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
 * \file    nfs3_Fsinfo.c
 * \author  $Author: deniel $
 * \date    $Date: 2005/11/28 17:02:49 $
 * \version $Revision: 1.11 $
 * \brief   Routines used for managing the NFS4 COMPOUND functions.
 *
 * nfs3_Fsinfo.c : Routines used for managing the NFS4 COMPOUND functions.
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
 * nfs3_Fsinfo: Implements NFSPROC3_FSINFO
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

int nfs3_Fsinfo( nfs_arg_t               * parg,    
                 exportlist_t            * pexport, 
                 fsal_op_context_t             * pcontext,   
                 cache_inode_client_t    * pclient,
                 hash_table_t            * ht,
                 struct svc_req          * preq,    
                 nfs_res_t               * pres )
{
	static char   __attribute__(( __unused__ ))     funcName[] = "nfs3_Fsinfo";

  cache_inode_status_t      cache_status ;
  cache_entry_t           * pentry = NULL ;
  cache_inode_fsal_data_t   fsal_data ;
  fsal_attrib_list_t        attr ;
  
  /* to avoid setting it on each error case */
  pres->res_fsinfo3.FSINFO3res_u.resfail.obj_attributes.attributes_follow = FALSE;

  /* Convert file handle into a fsal_handle */
  if( nfs3_FhandleToFSAL( &(parg->arg_fsinfo3.fsroot), &fsal_data.handle, pcontext ) == 0 )
	return NFS_REQ_DROP;

  /* Set the cookie */
  fsal_data.cookie = DIR_START ;

  /* Get the entry in the cache_inode */
  if( ( pentry = cache_inode_get( &fsal_data, 
                                  &attr, 
                                  ht, 
                                  pclient, 
                                  pcontext, 
                                  &cache_status ) ) == NULL )
    {
      /* Stale NFS FH ? */
      pres->res_fsinfo3.status = NFS3ERR_STALE ;
      return NFS_REQ_OK ;
    }
  
  
	/*
	 * New fields were added to nfs_config_t to handle this value. We use
	 * them 
	 */

#define FSINFO_FIELD pres->res_fsinfo3.FSINFO3res_u.resok
	FSINFO_FIELD.rtmax  = pexport->MaxRead;
	FSINFO_FIELD.rtpref = pexport->PrefRead;

	/* This field is generally unused, it will be removed in V4 */
	FSINFO_FIELD.rtmult = DEV_BSIZE;

	FSINFO_FIELD.wtmax  = pexport->MaxWrite;
	FSINFO_FIELD.wtpref = pexport->PrefWrite;

	/* This field is generally unused, it will be removed in V4 */
	FSINFO_FIELD.wtmult = DEV_BSIZE;

	FSINFO_FIELD.dtpref = pexport->PrefReaddir ;

	FSINFO_FIELD.maxfilesize = FSINFO_MAX_FILESIZE;
	FSINFO_FIELD.time_delta.seconds = 1;
	FSINFO_FIELD.time_delta.nseconds = 0;

#ifdef _DEBUG_NFSPROTO
	printf("rtmax = %d | rtpref = %d | trmult = %d\n",
	       FSINFO_FIELD.rtmax, FSINFO_FIELD.rtpref, FSINFO_FIELD.rtmult = DEV_BSIZE);
	printf("wtmax = %d | wtpref = %d | wrmult = %d\n",
	       FSINFO_FIELD.wtmax, FSINFO_FIELD.wtpref, FSINFO_FIELD.wtmult = DEV_BSIZE);
	printf("dtpref = %d | maxfilesize = %llu \n", FSINFO_FIELD.dtpref, FSINFO_FIELD.maxfilesize);
#endif


	/*
	 * Allow all kinds of operations to be performed on the server
	 * through NFS v3 
	 */
	FSINFO_FIELD.properties = FSF3_LINK | FSF3_SYMLINK | FSF3_HOMOGENEOUS | FSF3_CANSETTIME;

	nfs_SetPostOpAttr( pcontext, pexport,
                     pentry, 
                     &attr,
                     &(pres->res_fsinfo3.FSINFO3res_u.resok.obj_attributes));
	pres->res_fsinfo3.status = NFS3_OK;


	return NFS_REQ_OK ;
} /* nfs3_Fsinfo */


/**
 * nfs3_Fsinfo_Free: Frees the result structure allocated for nfs3_Fsinfo.
 * 
 * Frees the result structure allocated for nfs3_Fsinfo.
 * 
 * @param pres        [INOUT]   Pointer to the result structure.
 *
 */
void nfs3_Fsinfo_Free( nfs_res_t * pres )
{
  /* Nothing to do */
  return ;
} /* nfs3_Fsinfo_Free */
