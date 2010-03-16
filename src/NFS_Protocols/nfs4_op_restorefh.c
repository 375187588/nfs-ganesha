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
 * \file    nfs4_op_restorefh.c
 * \author  $Author: deniel $
 * \date    $Date: 2006/01/05 15:14:52 $
 * \version $Revision: 1.10 $
 * \brief   Routines used for managing the NFS4_OP_RESTOREFH operation (number 31).
 *
 * nfs4_op_restorefh.c : Routines used for managing the NFS4_OP_RESTOREFH operation.
 * 
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _SOLARIS
#include "solaris_port.h"
#endif

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>		/* for having FNDELAY */
#include "HashData.h"
#include "HashTable.h"
#ifdef _USE_GSSRPC
#include <gssrpc/types.h>
#include <gssrpc/rpc.h>
#include <gssrpc/auth.h>
#include <gssrpc/pmap_clnt.h>
#else
#include <rpc/types.h>
#include <rpc/rpc.h>
#include <rpc/auth.h>
#include <rpc/pmap_clnt.h>
#endif

#include "log_functions.h"
#include "stuff_alloc.h"
#include "nfs23.h"
#include "nfs4.h"
#include "mount.h"
#include "nfs_core.h"
#include "cache_inode.h"
#include "cache_content.h"
#include "nfs_exports.h"
#include "nfs_creds.h"
#include "nfs_proto_functions.h"
#include "nfs_tools.h"
#include "nfs_file_handle.h"

/**
 *
 * nfs4_op_restorefh: The NFS4_OP_RESTOREFH operation.
 *
 * This functions handles the NFS4_OP_RESTOREFH operation in NFSv4. This function can be called only from nfs4_Compound.
 * This operation replaces the current FH with the previously saved FH.
 *
 * @param op    [IN]    pointer to nfs4_op arguments
 * @param data  [INOUT] Pointer to the compound request's data
 * @param resp  [IN]    Pointer to nfs4_op results
 * 
 * @return NFS4_OK if successfull, other values show an error. 
 *
 * @see all the nfs4_op_<*> function
 * @see nfs4_Compound
 *
 */
int nfs4_op_restorefh(struct nfs_argop4 *op,
		      compound_data_t * data, struct nfs_resop4 *resp)
{
#ifdef _DEBUG_NFS_V4
  int i;
#endif
  int error;

  /* First of all, set the reply to zero to make sure it contains no parasite information */
  memset(resp, 0, sizeof(struct nfs_resop4));

  resp->resop = NFS4_OP_RESTOREFH;
  resp->nfs_resop4_u.oprestorefh.status = NFS4_OK;

  /* If there is no currentFH, teh  return an error */
  if (nfs4_Is_Fh_Empty(&(data->savedFH)))
    {
      /* There is no current FH, return NFS4ERR_RESTOREFH (cg RFC3530, page 202) */
      resp->nfs_resop4_u.oprestorefh.status = NFS4ERR_RESTOREFH;
      return resp->nfs_resop4_u.oprestorefh.status;
    }

  /* If the filehandle is invalid */
  if (nfs4_Is_Fh_Invalid(&(data->savedFH)))
    {
      resp->nfs_resop4_u.oprestorefh.status = NFS4ERR_BADHANDLE;
      return NFS4ERR_BADHANDLE;
    }

  /* Tests if teh Filehandle is expired (for volatile filehandle) */
  if (nfs4_Is_Fh_Expired(&(data->savedFH)))
    {
      resp->nfs_resop4_u.opgetfh.status = NFS4ERR_FHEXPIRED;
      return NFS4ERR_FHEXPIRED;
    }

  /* If data->exportp is null, a junction from pseudo fs was traversed, credp and exportp have to be updated */
  if (data->pexport == NULL)
    {
      if ((error = nfs4_SetCompoundExport(data)) != NFS4_OK)
	{
	  printf("Erreur %d dans nfs4_SetCompoundExport\n", error);
	  resp->nfs_resop4_u.opgetfh.status = error;
	  return resp->nfs_resop4_u.opgetfh.status;
	}
    }

  /* Copy the data from current FH to saved FH */
  memcpy((char *)(data->currentFH.nfs_fh4_val), (char *)(data->savedFH.nfs_fh4_val),
	 data->savedFH.nfs_fh4_len);

  data->current_entry = data->saved_entry;
  data->current_filetype = data->saved_filetype;

#ifdef _DEBUG_NFS_V4
  printf("CURRENTFH: File handle = { Length = %d  Val = ", data->currentFH.nfs_fh4_len);
  for (i = 0; i < data->currentFH.nfs_fh4_len; i++)
    printf("%02X", data->currentFH.nfs_fh4_val[i]);
  printf(" }\n");
#endif
  return NFS4_OK;
}				/* nfs4_op_restorefh */

/**
 * nfs4_op_restorefh_Free: frees what was allocared to handle nfs4_op_restorefh.
 * 
 * Frees what was allocared to handle nfs4_op_restorefh.
 *
 * @param resp  [INOUT]    Pointer to nfs4_op results
 *
 * @return nothing (void function )
 * 
 */
void nfs4_op_restorefh_Free(RESTOREFH4res * resp)
{
  /* Nothing to be done */
  return;
}				/* nfs4_op_restorefh_Free */
