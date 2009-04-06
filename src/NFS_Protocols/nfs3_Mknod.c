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
 * \file    nfs3_Mknod.c
 * \author  $Author: deniel $
 * \date    $Date: 2005/11/28 17:02:49 $
 * \version $Revision: 1.8 $
 * \brief   Routines used for managing the NFS4 COMPOUND functions.
 *
 * nfs3_Mknod.c : Routines used for managing the NFS4 COMPOUND functions.
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
#include <sys/file.h>  /* for having FNDELAY */
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
#include "nfs_proto_tools.h"
#include "nfs_tools.h"

/**
 * nfs3_Mknod: Implements NFSPROC3_MKNOD
 *
 * Implements NFSPROC3_COMMIT. Unused for now, but may be supported later. 
 * 
 * @param parg    [IN]    pointer to nfs arguments union
 * @param pexport [IN]    pointer to nfs export list 
 * @param pcontext   [IN]    credentials to be used for this request
 * @param pclient [INOUT] client resource to be used
 * @param ht      [INOUT] cache inode hash table
 * @param preq    [IN]    pointer to SVC request related to this call 
 * @param pres    [OUT]   pointer to the structure to contain the result of the call
 *
 * @return always NFS_REQ_OK (this routine does nothing)
 *
 */

int nfs3_Mknod( nfs_arg_t               * parg,    
                exportlist_t            * pexport, 
                fsal_op_context_t       * pcontext,   
                cache_inode_client_t    * pclient,
                hash_table_t            * ht,
                struct svc_req          * preq,    
                nfs_res_t               * pres )
{
    int rc;
    cache_entry_t         * parent_pentry = NULL ;
    fsal_attrib_list_t      parent_attr          ;
    fsal_attrib_list_t    * ppre_attr            ;
    fsal_attrib_list_t      attr_parent_after    ;
    cache_inode_file_type_t parent_filetype      ;
    cache_inode_file_type_t nodetype             ;
    char                  * str_file_name = NULL ;
    fsal_name_t             file_name            ;
    cache_inode_status_t    cache_status         ;
    cache_inode_status_t    cache_status_lookup  ;
    fsal_accessmode_t       mode = 0             ;
	cache_entry_t         * node_pentry = NULL   ;
    fsal_attrib_list_t      attr                 ;
    cache_inode_create_arg_t create_arg          ;
    fsal_handle_t         * pfsal_handle         ;
    
    /* to avoid setting them on each error case */
    
    pres->res_mknod3.MKNOD3res_u.resfail.dir_wcc.before.attributes_follow = FALSE;
    pres->res_mknod3.MKNOD3res_u.resfail.dir_wcc.after.attributes_follow = FALSE;
    ppre_attr = NULL;
    
    /* retrieve parent entry */  
  
    if( ( parent_pentry = nfs_FhandleToCache( preq->rq_vers, 
                                              NULL,
                                              &(parg->arg_mknod3.where.dir),
                                              NULL, 
                                              NULL,
                                              &(pres->res_mknod3.status ),
                                              NULL,
                                              &parent_attr, 
                                              pcontext, 
                                              pclient, 
                                              ht, 
                                              &rc ) ) == NULL )
    {
      /* Stale NFS FH ? */
      return rc ;
    }

    /* get directory attributes before action (for V3 reply) */
    ppre_attr = &parent_attr;

    /* Extract the filetype */
    parent_filetype = cache_inode_fsal_type_convert( parent_attr.type ) ;
  
    
	/*
	 * Sanity checks: new node name must be non-null; parent must be a
	 * directory. 
	 */
    if( ( parent_filetype != DIR_BEGINNING ) &&  ( parent_filetype != DIR_CONTINUE ) )
    {
      pres->res_mknod3.status = NFS3ERR_NOTDIR;
      return NFS_REQ_OK ;
	}
    
    str_file_name = parg->arg_mknod3.where.name;
    
	switch( parg->arg_mknod3.what.type )
	{
      case NF3CHR:
      case NF3BLK:
        
        if (parg->arg_mknod3.what.mknoddata3_u.device.dev_attributes.mode.set_it )
          mode = (fsal_accessmode_t)parg->arg_mknod3.what.mknoddata3_u.device.dev_attributes.mode.set_mode3_u.mode;
        else
          mode = (fsal_accessmode_t)0;
        
        create_arg.dev_spec.major = parg->arg_mknod3.what.mknoddata3_u.device.spec.specdata1;
        create_arg.dev_spec.minor = parg->arg_mknod3.what.mknoddata3_u.device.spec.specdata2;
        
        break;

      case NF3FIFO:
      case NF3SOCK:
        
        if (parg->arg_mknod3.what.mknoddata3_u.pipe_attributes.mode.set_it )
          mode = (fsal_accessmode_t)parg->arg_mknod3.what.mknoddata3_u.pipe_attributes.mode.set_mode3_u.mode;
        else
          mode = (fsal_accessmode_t)0;
        
        create_arg.dev_spec.major = 0;
        create_arg.dev_spec.minor = 0;
        
        break;
        
      default:
        pres->res_mknod3.status = NFS3ERR_BADTYPE;
        return NFS_REQ_OK ;
    }

  	switch( parg->arg_mknod3.what.type )
	{
      case NF3CHR:
        nodetype = CHARACTER_FILE;
        break;
      case NF3BLK:
        nodetype = BLOCK_FILE;
        break;
      case NF3FIFO:
        nodetype = FIFO_FILE;
        break;
      case NF3SOCK:
        nodetype = SOCKET_FILE;
        break;
      default:
        pres->res_mknod3.status = NFS3ERR_BADTYPE;
        return NFS_REQ_OK ;
    }
        
    
    if( str_file_name == NULL || strlen(str_file_name) == 0)
	{
       pres->res_mknod3.status = NFS3ERR_INVAL;
       return NFS_REQ_OK ;
    }
       
       
    /* convert node name */

    if( ( cache_status = cache_inode_error_convert( FSAL_str2name( str_file_name, 
                                                                   FSAL_MAX_NAME_LEN,
                                                                   &file_name ) ) ) == CACHE_INODE_SUCCESS )
      {
        /*
         * Lookup node to see if it exists.  If so, use it.  Otherwise
         * create a new one.
         */      
        node_pentry = cache_inode_lookup( parent_pentry, 
                                          &file_name, 
                                          &attr, 
                                          ht, 
                                          pclient, 
                                          pcontext,
                                          &cache_status_lookup ) ;

        if( cache_status_lookup == CACHE_INODE_NOT_FOUND )
          {

            /* Create the node */

            if( ( node_pentry = cache_inode_create( parent_pentry, 
                                                   &file_name,
                                                   nodetype, 
                                                   mode, 
                                                   &create_arg, 
                                                   &attr,
                                                   ht, 
                                                   pclient, 
                                                   pcontext,
                                                   &cache_status ) ) != NULL )
              {

                  /*
                   * Get the FSAL handle for this entry 
                   */
                  pfsal_handle =  cache_inode_get_fsal_handle( node_pentry, 
                                                               &cache_status ) ;
                  
                  if( cache_status == CACHE_INODE_SUCCESS )
                  {
                    
#ifdef _DEBUG_MEMLEAKS
                    /* For debugging memory leaks */
                    BuddySetDebugLabel( "Filehandle V3 in nfs3_mknod" ) ;
#endif  

                    /* Build file handle */
                    if( ( pres->res_mknod3.MKNOD3res_u.resok.obj.post_op_fh3_u.handle.data.data_val = Mem_Alloc( NFS3_FHSIZE ) ) == NULL )
                      {
                        pres->res_mknod3.status = NFS3ERR_IO ; 
                        return NFS_REQ_OK ;
                      }

#ifdef _DEBUG_MEMLEAKS
                    /* For debugging memory leaks */
                    BuddySetDebugLabel( "N/A" ) ;
#endif   

                    if( nfs3_FSALToFhandle( &pres->res_mknod3.MKNOD3res_u.resok.obj.post_op_fh3_u.handle,
                                            pfsal_handle, 
                                            pexport ) == 0 )
                      {
                        Mem_Free( (char *)pres->res_mknod3.MKNOD3res_u.resok.obj.post_op_fh3_u.handle.data.data_val ) ;
                        pres->res_mknod3.status = NFS3ERR_INVAL;
                        return NFS_REQ_OK ;
                      }
                    else
                      { 
                        /* Set Post Op Fh3 structure */
                        pres->res_mknod3.MKNOD3res_u.resok.obj.handle_follows = TRUE ;
                        pres->res_mknod3.MKNOD3res_u.resok.obj.post_op_fh3_u.handle.data.data_len  =  sizeof( file_handle_v3_t ) ;

                        /*
                         * Build entry
                         * attributes 
                         */
                        nfs_SetPostOpAttr( pcontext, pexport,
                                           node_pentry, 
                                           &attr,
                                           &(pres->res_mknod3.MKNOD3res_u.resok.obj_attributes));


                        /* Get the attributes of the parent after the operation */
                        cache_inode_get_attributes( parent_pentry, &attr_parent_after ) ;

                        /*
                         * Build Weak Cache
                         * Coherency data 
                         */
                        nfs_SetWccData( pcontext, pexport,
                                        parent_pentry, 
                                        ppre_attr, 
                                        &attr_parent_after,
                                        &(pres->res_mknod3.MKNOD3res_u.resok.dir_wcc));

                        pres->res_mknod3.status = NFS3_OK;
                      }
                     
                     
                    
                    return NFS_REQ_OK ;

                  } /* get fsal handle success */
                  
              } /* mknod sucess */

          } /* not found */
        else
          {
            /* object already exists or failure during lookup */
            if( cache_status_lookup == CACHE_INODE_SUCCESS )
              {
                /* Trying to create an entry that already exists */
                cache_status =  CACHE_INODE_ENTRY_EXISTS ;                  
                pres->res_mknod3.status = NFS3ERR_EXIST ;
              }
            else
              {
                /* Server fault */
                cache_status = cache_status_lookup ;
                pres->res_mknod3.status = NFS3ERR_INVAL ;
              }

            nfs_SetFailedStatus( pcontext, pexport,
                                 preq->rq_vers, 
                                 cache_status,
                                 NULL,
                                 &pres->res_mknod3.status,
                                 NULL, NULL,
                                 parent_pentry,
                                 ppre_attr,
                                 &(pres->res_mknod3.MKNOD3res_u.resfail.dir_wcc),
                                 NULL, NULL, NULL);

            return NFS_REQ_OK ;
          }


      } /* convertion OK */

    
	/* If we are here, there was an error */
	if( nfs_RetryableError( cache_status ) )
    {
      return NFS_REQ_DROP ;
      
    }
	nfs_SetFailedStatus( pcontext, pexport,
                       preq->rq_vers, 
                       cache_status,
                       NULL,
                       &pres->res_mknod3.status,
                       NULL, NULL,
                       parent_pentry,
                       ppre_attr,
                       &(pres->res_mknod3.MKNOD3res_u.resfail.dir_wcc),
                       NULL, NULL, NULL);

	return NFS_REQ_OK ;
    
} /* nfs3_Mknod */

/**
 * nfs3_Mknod_Free: Frees the result structure allocated for nfs3_Mknod.
 * 
 * Frees the result structure allocated for nfs3_Mknod.
 * 
 * @param pres        [INOUT]   Pointer to the result structure.
 *
 */
void nfs3_Mknod_Free( nfs_res_t * pres )
{
    if ((pres->res_mknod3.status == NFS3_OK) &&
        (pres->res_mknod3.MKNOD3res_u.resok.obj.handle_follows == TRUE))
      Mem_Free(pres->res_mknod3.MKNOD3res_u.resok.obj.post_op_fh3_u.handle.data.data_val);

} /* nfs3_Mknod_Free */


