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
 * \file    nfs4_op_close.c
 * \author  $Author: deniel $
 * \date    $Date: 2005/11/28 17:02:50 $
 * \version $Revision: 1.8 $
 * \brief   Routines used for managing the NFS4 COMPOUND functions.
 *
 * nfs4_op_close.c : Routines used for managing the NFS4 COMPOUND functions.
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
#include "nfs_tools.h"
#include "nfs_file_handle.h"


/**
 *
 * nfs4_op_close: Implemtation of NFS4_OP_CLOSE
 * 
 * Implemtation of NFS4_OP_CLOSE. Implementation is partial for now, so it always returns NFS4_OK.  
 *
 * @param op    [IN]    pointer to nfs4_op arguments
 * @param data  [INOUT] Pointer to the compound request's data
 * @param resp  [IN]    Pointer to nfs4_op results
 * 
 * @return NFS4_OK 
 * 
 */

#define arg_CLOSE4 op->nfs_argop4_u.opclose
#define res_CLOSE4 resp->nfs_resop4_u.opclose

int nfs4_op_close(  struct nfs_argop4 * op ,   
                    compound_data_t   * data,
                    struct nfs_resop4 * resp)
{
  int                    rc = 0 ;
  char                   __attribute__(( __unused__ )) funcname[] = "nfs4_op_close" ;
  cache_inode_state_t  * pstate_found = NULL ; 

  cache_inode_status_t   cache_status ;
  
  memset( &res_CLOSE4, 0, sizeof( res_CLOSE4 ) ) ;
  resp->resop = NFS4_OP_CLOSE ;
 
   /* If the filehandle is Empty */
  if( nfs4_Is_Fh_Empty( &(data->currentFH  ) ) )
    {
      res_CLOSE4.status = NFS4ERR_NOFILEHANDLE ;
      return res_CLOSE4.status ;
    }

  /* If the filehandle is invalid */
  if( nfs4_Is_Fh_Invalid( &(data->currentFH ) ) )
    {
      res_CLOSE4.status = NFS4ERR_BADHANDLE ;
      return res_CLOSE4.status ;
    }

  /* Tests if the Filehandle is expired (for volatile filehandle) */
  if( nfs4_Is_Fh_Expired( &(data->currentFH) ) )
    {
      res_CLOSE4.status = NFS4ERR_FHEXPIRED ;
      return res_CLOSE4.status ;
    }

  if( data->current_entry == NULL )
   {
      res_CLOSE4.status = NFS4ERR_SERVERFAULT ;
      return res_CLOSE4.status ;
   }

  /* Should not operate on directories */
  if( data->current_entry->internal_md.type == DIR_BEGINNING ||
      data->current_entry->internal_md.type == DIR_CONTINUE )
    {
       res_CLOSE4.status = NFS4ERR_ISDIR ;
       return res_CLOSE4.status ;
    }

  /* Object should be a file */
  if( data->current_entry->internal_md.type != REGULAR_FILE )
    {
        res_CLOSE4.status = NFS4ERR_INVAL ;
        return res_CLOSE4.status ;
    }

#ifdef _TOTO_BUGAZOMEU /** @todo : BUGAZOMEU: check for lease use */
  /* Check for expiration of the lease */
  if( nfs4_is_lease_expired( data->current_entry ) )
    {
       res_CLOSE4.status = NFS4ERR_EXPIRED ;
       return res_CLOSE4.status ;
    }
#endif

  /* Does the stateid match ? */
  if( ( rc = nfs4_Check_Stateid( &arg_CLOSE4.open_stateid, data->current_entry ) )  != NFS4_OK )
    {
	res_CLOSE4.status = rc ;
	return res_CLOSE4.status ;
    }


  /* Get the related state */
  if( cache_inode_get_state( arg_CLOSE4.open_stateid.other,
                             &pstate_found,
                             data->pclient,
                             &cache_status ) != CACHE_INODE_SUCCESS )
    {
       if( cache_status == CACHE_INODE_NOT_FOUND )
          res_CLOSE4.status = NFS4ERR_BAD_STATEID ;
       else
          res_CLOSE4.status = NFS4ERR_INVAL  ;
           
        printf( "====> Sortie A\n" ) ;
       return res_CLOSE4.status ;
    }

  /* Check is held locks remain */
  if(  pstate_found->state_data.share.lockheld > 0 )
   {
      res_CLOSE4.status = NFS4ERR_LOCKS_HELD ;
      return res_CLOSE4.status ;
   }

  /* Check the seqid */
  if( ( arg_CLOSE4.seqid != pstate_found->seqid ) &&
      ( arg_CLOSE4.seqid != pstate_found->seqid + 1 ) )
     {
	res_CLOSE4.status = NFS4ERR_BAD_SEQID ;
	return res_CLOSE4.status ;
     }

  /* Prepare the result */
  res_CLOSE4.CLOSE4res_u.open_stateid.seqid = pstate_found->seqid + 1 ;
  memcpy( res_CLOSE4.CLOSE4res_u.open_stateid.other, arg_CLOSE4.open_stateid.other , 12 ) ;  ;

  /* File is closed, release the corresponding state */
  if( cache_inode_del_state_by_key( arg_CLOSE4.open_stateid.other,
                                    data->pclient, 
                                    &cache_status ) != CACHE_INODE_SUCCESS ) 
    {
	res_CLOSE4.status = nfs4_Errno( cache_status ) ;
	return res_CLOSE4.status ;
    }

  /* Close the file in FSAL through the cache inode */
  P_w( &data->current_entry->lock ) ;
  if( cache_inode_close( data->current_entry,
                         data->pclient,
                         &cache_status ) != CACHE_INODE_SUCCESS )
    {
       V_w( &data->current_entry->lock ) ;

       res_CLOSE4.status = nfs4_Errno( cache_status ) ;
       return res_CLOSE4.status ;
    }
  V_w( &data->current_entry->lock ) ;

  res_CLOSE4.status = NFS4_OK ;
  
  return NFS4_OK ;
} /* nfs4_op_close */


/**
 * nfs4_op_close_Free: frees what was allocared to handle nfs4_op_close.
 * 
 * Frees what was allocared to handle nfs4_op_close.
 *
 * @param resp  [INOUT]    Pointer to nfs4_op results
 *
 * @return nothing (void function )
 * 
 */
void nfs4_op_close_Free( CLOSE4res * resp )
{
  /* Nothing to be done */
  return ;
} /* nfs4_op_close_Free */

