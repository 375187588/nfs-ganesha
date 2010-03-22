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
 * \file    cache_inode_renew_entry.c
 * \author  $Author: leibovic $
 * \date    $Date: 2006/02/16 08:22:29 $
 * \version $Revision: 1.13 $
 * \brief   Renews an entry in the cache inode.
 *
 * cache_inode_renew_entry.c : Renews an entry in the cache inode.
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
#include "stuff_alloc.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <time.h>
#include <pthread.h>

/**
 *
 * cache_inode_renew_entry: Renews the attributes for an entry.
 * 
 * Sets the attributes for an entry located in the cache by its address. Attributes are provided 
 * with compliance to the underlying FSAL semantics. Attributes that are set are returned in "*pattr".
 *
 * @param pentry_parent [IN] entry for the parent directory to be managed.
 * @param pattr [OUT] renewed attributes for the entry that we have found. 
 * @param pclient [INOUT] ressource allocated by the client for the nfs management.
 * @param pcontext [IN] FSAL credentials 
 * @param pstatus [OUT] returned status.
 * 
 * @return CACHE_INODE_SUCCESS if operation is a success \n
   @return Other errors shows a FSAL error.
 *
 */
cache_inode_status_t cache_inode_renew_entry(cache_entry_t * pentry,
                                             fsal_attrib_list_t * pattr,
                                             hash_table_t * ht,
                                             cache_inode_client_t * pclient,
                                             fsal_op_context_t * pcontext,
                                             cache_inode_status_t * pstatus)
{
  fsal_handle_t *pfsal_handle = NULL;
  fsal_status_t fsal_status;
  fsal_attrib_list_t object_attributes;
  fsal_path_t link_content;
  time_t current_time = time(NULL);
  time_t entry_time = pentry->internal_md.refresh_time;

  /* If we do nothing (no expiration) then everything is all right */
  *pstatus = CACHE_INODE_SUCCESS;

#ifdef _DEBUG_CACHE_INODE
  DisplayLogLevel(NIV_FULL_DEBUG,
                  "Entry=%p, type=%d, current=%d, read=%d, refresh=%d, alloc=%d",
                  pentry, pentry->internal_md.type, current_time,
                  pentry->internal_md.read_time, pentry->internal_md.refresh_time,
                  pentry->internal_md.alloc_time);
#endif

  /* An entry that is a regular file with an associated File Content Entry won't
   * expire until data exists in File Content Cache, to avoid attributes incoherency */

  /* @todo: BUGAZOMEU: I got serious doubts on the following blocks: possible trouble if using data caching */
  if (pentry->internal_md.type == REGULAR_FILE &&
      pentry->object.file.pentry_content != NULL)
    {
      /* Successfull exit without having nothing to do ... */

      DisplayLogJdLevel(pclient->log_outputs, NIV_DEBUG,
                        "Entry %p is a REGULAR_FILE with associated data cached %p, no expiration",
                        pentry, pentry->object.file.pentry_content);

      *pstatus = CACHE_INODE_SUCCESS;
      return *pstatus;
    }

  /* Do we use getattr/mtime checking */
  if (pclient->getattr_dir_invalidation &&
      pentry->internal_md.type == DIR_BEGINNING &&
      FSAL_TEST_MASK(pclient->attrmask, FSAL_ATTR_MTIME) &&
      pentry->object.dir_begin.has_been_readdir == CACHE_INODE_YES)
    {
      /* This checking is to be done ... */
      pfsal_handle = &pentry->object.dir_begin.handle;

      /* Call FSAL to get the attributes */
      object_attributes.asked_attributes = pclient->attrmask;

      fsal_status = FSAL_getattrs(pfsal_handle, pcontext, &object_attributes);

      if (FSAL_IS_ERROR(fsal_status))
        {
          *pstatus = cache_inode_error_convert(fsal_status);

          if (fsal_status.major == ERR_FSAL_STALE)
            {
              cache_inode_status_t kill_status;

              DisplayLog
                  ("cache_inode_renew_entry: Stale FSAL File Handle detected for pentry = %p, line %u",
                   pentry, __LINE__);

              if (cache_inode_kill_entry(pentry, ht, pclient, &kill_status) !=
                  CACHE_INODE_SUCCESS)
                DisplayLog
                    ("cache_inode_renew_entry: Could not kill entry %p, status = %u",
                     pentry, kill_status);

              *pstatus = CACHE_INODE_FSAL_ESTALE;
            }
          /* stat */
          pclient->stat.func_stats.nb_err_unrecover[CACHE_INODE_RENEW_ENTRY] += 1;

          return *pstatus;
        }
#ifdef  _DEBUG_CACHE_INODE
      DisplayLogJdLevel(pclient->log_outputs, NIV_FULL_DEBUG,
                        "Entry=%p, type=%d, Cached Time=%d, FSAL Time=%d",
                        pentry, pentry->internal_md.type,
                        pentry->object.dir_begin.attributes.mtime.seconds,
                        object_attributes.mtime.seconds);
#endif

      /* Compare the FSAL mtime and the cached mtime */
      if (pentry->object.dir_begin.attributes.mtime.seconds <
          object_attributes.mtime.seconds)
        {
          /* Cached directory content is obsolete, it must be renewed */
          pentry->object.dir_begin.attributes = object_attributes;

          /* Return the attributes as set */
          if (pattr != NULL)
            *pattr = object_attributes;

          /* Set the directory content as "to be renewed" */
          /* Next call to cache_inode_readdir will repopulate the dirent array */
          pentry->object.dir_begin.has_been_readdir = CACHE_INODE_RENEW_NEEDED;

          /* Set the refresh time for the cache entry */
          pentry->internal_md.refresh_time = time(NULL);

          DisplayLogJdLevel(pclient->log_outputs, NIV_DEBUG,
                            "cached directory content for entry %p must be renewed, due to getattr mismatch",
                            pentry);

        }                       /* if( pentry->object.dir_begin.attributes.mtime < object_attributes.asked_attributes.mtime ) */
    }

  /* if( pclient->getattr_dir_invalidation && ... */
  /* Check for dir content expiration */
  if (pentry->internal_md.type == DIR_BEGINNING &&
      pclient->grace_period_dirent != 0 &&
      pentry->object.dir_begin.has_been_readdir == CACHE_INODE_YES &&
      (current_time - entry_time > pclient->grace_period_dirent))
    {
      /* stat */
      pclient->stat.func_stats.nb_call[CACHE_INODE_RENEW_ENTRY] += 1;

      /* Log */
#ifdef  _DEBUG_CACHE_INODE
      DisplayLogJdLevel(pclient->log_outputs, NIV_FULL_DEBUG,
                        "Entry=%p, type=%d, Time=%d, current=%d, grace_period_dirent=%d",
                        pentry, pentry->internal_md.type,
                        entry_time, current_time, pclient->grace_period_dirent);

      DisplayLogJdLevel(pclient->log_outputs, NIV_FULL_DEBUG,
                        "cached directory entries for entry %p must be renewed", pentry);
#endif

      /* Do the getattr if it had not being done before */
      if (pfsal_handle == NULL)
        {
          pfsal_handle = &pentry->object.dir_begin.handle;

          /* Call FSAL to get the attributes */
          object_attributes.asked_attributes = pclient->attrmask;

          fsal_status = FSAL_getattrs(pfsal_handle, pcontext, &object_attributes);

          if (FSAL_IS_ERROR(fsal_status))
            {
              *pstatus = cache_inode_error_convert(fsal_status);

              /* stat */
              pclient->stat.func_stats.nb_err_unrecover[CACHE_INODE_RENEW_ENTRY] += 1;

              if (fsal_status.major == ERR_FSAL_STALE)
                {
                  cache_inode_status_t kill_status;

                  DisplayLog
                      ("cache_inode_renew_entry: Stale FSAL File Handle detected for pentry = %p, line %u",
                       pentry, __LINE__);

                  if (cache_inode_kill_entry(pentry, ht, pclient, &kill_status) !=
                      CACHE_INODE_SUCCESS)
                    DisplayLog
                        ("cache_inode_renew_entry: Could not kill entry %p, status = %u",
                         pentry, kill_status);

                  *pstatus = CACHE_INODE_FSAL_ESTALE;
                }

              return *pstatus;
            }
        }

      pentry->object.dir_begin.attributes = object_attributes;

      /* Return the attributes as set */
      if (pattr != NULL)
        *pattr = object_attributes;

      /* Set the directory content as "to be renewed" */
      /* Next call to cache_inode_readdir will repopulate the dirent array */
      pentry->object.dir_begin.has_been_readdir = CACHE_INODE_RENEW_NEEDED;

      /* Set the refresh time for the cache entry */
      pentry->internal_md.refresh_time = time(NULL);

    }

  /* if( pentry->internal_md.type == DIR_BEGINNING && ... */
  /* if the directory has not been readdir, only update its attributes */
  else if (pentry->internal_md.type == DIR_BEGINNING &&
           pclient->grace_period_attr != 0 &&
           pentry->object.dir_begin.has_been_readdir != CACHE_INODE_YES &&
           (current_time - entry_time > pclient->grace_period_attr))
    {
      /* stat */
      pclient->stat.func_stats.nb_call[CACHE_INODE_RENEW_ENTRY] += 1;

      /* Log */
#ifdef  _DEBUG_CACHE_INODE
      DisplayLogJdLevel(pclient->log_outputs, NIV_DEBUG,
                        "Entry=%p, type=%d, Time=%d, current=%d, grace_period_dirent=%d",
                        pentry, pentry->internal_md.type,
                        entry_time, current_time, pclient->grace_period_dirent);

      DisplayLogJdLevel(pclient->log_outputs, NIV_DEBUG,
                        "cached directory entries for entry %p must be renewed", pentry);
#endif

      pfsal_handle = &pentry->object.dir_begin.handle;

      /* Call FSAL to get the attributes */
      object_attributes.asked_attributes = pclient->attrmask;

      fsal_status = FSAL_getattrs(pfsal_handle, pcontext, &object_attributes);

      if (FSAL_IS_ERROR(fsal_status))
        {
          *pstatus = cache_inode_error_convert(fsal_status);

          /* stat */
          pclient->stat.func_stats.nb_err_unrecover[CACHE_INODE_RENEW_ENTRY] += 1;

          if (fsal_status.major == ERR_FSAL_STALE)
            {
              cache_inode_status_t kill_status;

              DisplayLog
                  ("cache_inode_renew_entry: Stale FSAL File Handle detected for pentry = %p, line %u",
                   pentry, __LINE__);

              if (cache_inode_kill_entry(pentry, ht, pclient, &kill_status) !=
                  CACHE_INODE_SUCCESS)
                DisplayLog
                    ("cache_inode_renew_entry: Could not kill entry %p, status = %u",
                     pentry, kill_status);

              *pstatus = CACHE_INODE_FSAL_ESTALE;
            }

          return *pstatus;
        }

      pentry->object.dir_begin.attributes = object_attributes;

      /* Return the attributes as set */
      if (pattr != NULL)
        *pattr = object_attributes;

      /* Set the refresh time for the cache entry */
      pentry->internal_md.refresh_time = time(NULL);

    }

  /* else if( pentry->internal_md.type == DIR_BEGINNING && ... */
  /* Check for attributes expiration in other cases */
  else if (pentry->internal_md.type != DIR_CONTINUE &&
           pentry->internal_md.type != DIR_BEGINNING &&
           pclient->grace_period_attr != 0 &&
           (current_time - entry_time > pclient->grace_period_attr))
    {
      /* stat */
      pclient->stat.func_stats.nb_call[CACHE_INODE_RENEW_ENTRY] += 1;

      /* Log */
      DisplayLogJdLevel(pclient->log_outputs, NIV_DEBUG,
                        "Entry=%p, type=%d, Time=%d, current=%d, grace_period_attr=%d",
                        pentry, pentry->internal_md.type,
                        entry_time, current_time, pclient->grace_period_attr);

      DisplayLogJdLevel(pclient->log_outputs, NIV_DEBUG,
                        "Attributes for entry %p must be renewed", pentry);

      switch (pentry->internal_md.type)
        {
        case REGULAR_FILE:
          pfsal_handle = &pentry->object.file.handle;
          break;

        case SYMBOLIC_LINK:
          pfsal_handle = &pentry->object.symlink.handle;
          break;

        case SOCKET_FILE:
        case FIFO_FILE:
        case CHARACTER_FILE:
        case BLOCK_FILE:
          pfsal_handle = &pentry->object.special_obj.handle;
          break;
        }

      /* Call FSAL to get the attributes */
      object_attributes.asked_attributes = pclient->attrmask;
      fsal_status = FSAL_getattrs(pfsal_handle, pcontext, &object_attributes);
      if (FSAL_IS_ERROR(fsal_status))
        {
          *pstatus = cache_inode_error_convert(fsal_status);

          /* stat */
          pclient->stat.func_stats.nb_err_unrecover[CACHE_INODE_RENEW_ENTRY] += 1;

          if (fsal_status.major == ERR_FSAL_STALE)
            {
              cache_inode_status_t kill_status;

              DisplayLog
                  ("cache_inode_renew_entry: Stale FSAL File Handle detected for pentry = %p, line %u",
                   pentry, __LINE__);

              if (cache_inode_kill_entry(pentry, ht, pclient, &kill_status) !=
                  CACHE_INODE_SUCCESS)
                DisplayLog
                    ("cache_inode_renew_entry: Could not kill entry %p, status = %u",
                     pentry, kill_status);

              *pstatus = CACHE_INODE_FSAL_ESTALE;
            }

          return *pstatus;
        }

      /* Keep the new attribute in cache */
      switch (pentry->internal_md.type)
        {
        case REGULAR_FILE:
          pentry->object.file.attributes = object_attributes;
          break;

        case SYMBOLIC_LINK:
          pentry->object.symlink.attributes = object_attributes;
          break;

        case SOCKET_FILE:
        case FIFO_FILE:
        case CHARACTER_FILE:
        case BLOCK_FILE:
          pentry->object.special_obj.attributes = object_attributes;
          break;
        }

      /* Return the attributes as set */
      if (pattr != NULL)
        *pattr = object_attributes;

      /* Set the refresh time for the cache entry */
      pentry->internal_md.refresh_time = time(NULL);

    }

  /* if(  pentry->internal_md.type   != DIR_CONTINUE && ... */
  /* Check for link content expiration */
  if (pentry->internal_md.type == SYMBOLIC_LINK &&
      pclient->grace_period_link != 0 &&
      (current_time - entry_time > pclient->grace_period_link))
    {
      pfsal_handle = &pentry->object.symlink.handle;

      /* TMP DEBUG */
      DisplayLogJd(pclient->log_outputs,
                   "Entry=%p, type=%d, Time=%d, current=%d, grace_period_link=%d", pentry,
                   pentry->internal_md.type, entry_time, current_time,
                   pclient->grace_period_link);

      /* Log */
      DisplayLogJdLevel(pclient->log_outputs, NIV_DEBUG,
                        "Entry=%p, type=%d, Time=%d, current=%d, grace_period_link=%d",
                        pentry, pentry->internal_md.type,
                        entry_time, current_time, pclient->grace_period_link);
      DisplayLogJdLevel(pclient->log_outputs, NIV_DEBUG,
                        "cached link content for entry %p must be renewed", pentry);

      FSAL_CLEAR_MASK(object_attributes.asked_attributes);
      FSAL_SET_MASK(object_attributes.asked_attributes, pclient->attrmask);

      fsal_status =
          FSAL_readlink(pfsal_handle, pcontext, &link_content, &object_attributes);
      if (FSAL_IS_ERROR(fsal_status))
        {
          *pstatus = cache_inode_error_convert(fsal_status);
          /* stats */
          pclient->stat.func_stats.nb_err_unrecover[CACHE_INODE_RENEW_ENTRY] += 1;

          if (fsal_status.major == ERR_FSAL_STALE)
            {
              cache_inode_status_t kill_status;

              DisplayLog
                  ("cache_inode_renew_entry: Stale FSAL File Handle detected for pentry = %p, line %u",
                   pentry, __LINE__);

              if (cache_inode_kill_entry(pentry, ht, pclient, &kill_status) !=
                  CACHE_INODE_SUCCESS)
                DisplayLog
                    ("cache_inode_renew_entry: Could not kill entry %p, status = %u",
                     pentry, kill_status);

              *pstatus = CACHE_INODE_FSAL_ESTALE;
            }

        }
      else
        {
          fsal_status = FSAL_pathcpy(&pentry->object.symlink.content, &link_content);
          if (FSAL_IS_ERROR(fsal_status))
            {
              *pstatus = cache_inode_error_convert(fsal_status);
              /* stats */
              pclient->stat.func_stats.nb_err_unrecover[CACHE_INODE_RENEW_ENTRY] += 1;
            }
        }

      /* Set the refresh time for the cache entry */
      pentry->internal_md.refresh_time = time(NULL);

    }

  /* if( pentry->internal_md.type == SYMBOLIC_LINK && ... */
  return *pstatus;
}                               /* cache_inode_renew_entry */
