/*
 * vim:expandtab:shiftwidth=8:tabstop=8:
 *
 * Copyright CEA/DAM/DIF  (2008)
 * contributeur : Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
 *
 *
 * Ce logiciel est un serveur implementant le protocole NFS.
 *
 * Ce logiciel est r�gi par la licence CeCILL soumise au droit fran�ais et
 * respectant les principes de diffusion des logiciels libres. Vous pouvez
 * utiliser, modifier et/ou redistribuer ce programme sous les conditions
 * de la licence CeCILL telle que diffus�e par le CEA, le CNRS et l'INRIA
 * sur le site "http://www.cecill.info".
 *
 * En contrepartie de l'accessibilit� au code source et des droits de copie,
 * de modification et de redistribution accord�s par cette licence, il n'est
 * offert aux utilisateurs qu'une garantie limit�e.  Pour les m�mes raisons,
 * seule une responsabilit� restreinte p�se sur l'auteur du programme,  le
 * titulaire des droits patrimoniaux et les conc�dants successifs.
 *
 * A cet �gard  l'attention de l'utilisateur est attir�e sur les risques
 * associ�s au chargement,  � l'utilisation,  � la modification et/ou au
 * d�veloppement et � la reproduction du logiciel par l'utilisateur �tant
 * donn� sa sp�cificit� de logiciel libre, qui peut le rendre complexe �
 * manipuler et qui le r�serve donc � des d�veloppeurs et des professionnels
 * avertis poss�dant  des  connaissances  informatiques approfondies.  Les
 * utilisateurs sont donc invit�s � charger  et  tester  l'ad�quation  du
 * logiciel � leurs besoins dans des conditions permettant d'assurer la
 * s�curit� de leurs syst�mes et ou de leurs donn�es et, plus g�n�ralement,
 * � l'utiliser et l'exploiter dans les m�mes conditions de s�curit�.
 *
 * Le fait que vous puissiez acc�der � cet en-t�te signifie que vous avez
 * pris connaissance de la licence CeCILL, et que vous en avez accept� les
 * termes.
 *
 * ---------------------
 *
 * Copyright CEA/DAM/DIF (2005)
 *  Contributor: Philippe DENIEL  philippe.deniel@cea.fr
 *               Thomas LEIBOVICI thomas.leibovici@cea.fr
 *
 *
 * This software is a server that implements the NFS protocol.
 * 
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 * ---------------------------------------
 */

/**
 * \file    cache_inode_readlink.c
 * \author  $Author: deniel $
 * \date    $Date: 2005/12/05 09:02:36 $
 * \version $Revision: 1.16 $
 * \brief   Reads a symlink.
 *
 * cache_inode_readlink.c : Reads a symlink.
 *
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "LRU_List.h"
#include "log_functions.h"
#include "HashData.h"
#include "HashTable.h"
#include "fsal.h"
#include "cache_inode.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <time.h>
#include <pthread.h>

cache_inode_status_t cache_inode_readlink(cache_entry_t * pentry, fsal_path_t * plink_content, hash_table_t * ht,	/* Unused, kept for protototype's homogeneity */
					  cache_inode_client_t * pclient,
					  fsal_op_context_t * pcontext,
					  cache_inode_status_t * pstatus)
{
  fsal_status_t fsal_status;

  /* Sanity Check */
  if (pentry == NULL || pclient == NULL || pcontext == NULL || pstatus == NULL)
    {
      if (pstatus != NULL)
	*pstatus = CACHE_INODE_INVALID_ARGUMENT;
      return CACHE_INODE_INVALID_ARGUMENT;
    }

  /* Entry should not be dead */
  if (pentry->async_health != CACHE_INODE_ASYNC_STAYING_ALIVE)
    {
      *pstatus = CACHE_INODE_DEAD_ENTRY;
      return *pstatus;
    }

  /* Set the return default to CACHE_INODE_SUCCESS */
  *pstatus = CACHE_INODE_SUCCESS;

  /* stats */
  pclient->stat.nb_call_total += 1;
  pclient->stat.func_stats.nb_call[CACHE_INODE_READLINK] += 1;

  /* Lock the entry */
  P(pentry->lock);

  if (cache_inode_renew_entry(pentry, NULL, ht, pclient, pcontext, pstatus) !=
      CACHE_INODE_SUCCESS)
    {
      pclient->stat.func_stats.nb_err_retryable[CACHE_INODE_READLINK] += 1;
      V(pentry->lock);
      return *pstatus;
    }

  switch (pentry->internal_md.type)
    {
    case REGULAR_FILE:
    case DIR_BEGINNING:
    case DIR_CONTINUE:
    case CHARACTER_FILE:
    case BLOCK_FILE:
    case SOCKET_FILE:
    case FIFO_FILE:
      *pstatus = CACHE_INODE_BAD_TYPE;
      V(pentry->lock);

      /* stats */
      pclient->stat.func_stats.nb_err_unrecover[CACHE_INODE_READLINK] += 1;

      return *pstatus;
      break;

    case SYMBOLIC_LINK:
      fsal_status = FSAL_pathcpy(plink_content, &(pentry->object.symlink.content));
      if (FSAL_IS_ERROR(fsal_status))
	{
	  *pstatus = cache_inode_error_convert(fsal_status);
	  V(pentry->lock);

	  if (fsal_status.major == ERR_FSAL_STALE)
	    {
	      cache_inode_status_t kill_status;

	      DisplayLog
		  ("cache_inode_readlink: Stale FSAL File Handle detected for pentry = %p",
		   pentry);

	      if (cache_inode_kill_entry(pentry, ht, pclient, &kill_status) !=
		  CACHE_INODE_SUCCESS)
		DisplayLog("cache_inode_readlink: Could not kill entry %p, status = %u",
			   pentry, kill_status);

	      *pstatus = CACHE_INODE_FSAL_ESTALE;
	    }
	  /* stats */
	  pclient->stat.func_stats.nb_err_unrecover[CACHE_INODE_READLINK] += 1;

	  return *pstatus;
	}

      break;
    }

  /* Release the entry */
  *pstatus = cache_inode_valid(pentry, CACHE_INODE_OP_GET, pclient);
  V(pentry->lock);

  /* stat */
  if (*pstatus != CACHE_INODE_SUCCESS)
    pclient->stat.func_stats.nb_err_retryable[CACHE_INODE_READLINK] += 1;
    else
    pclient->stat.func_stats.nb_success[CACHE_INODE_READLINK] += 1;

  return *pstatus;
}				/* cache_inode_readlink */
