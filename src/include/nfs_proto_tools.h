/*
 *
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
 *
 * \file nfs_proto_tools.c
 * \author  $Author: deniel $
 * \date    $Date: 2005/11/28 17:03:23 $
 * \version $Revision: 1.9 $
 * \brief   A set of functions used to managed NFS.
 *
 * nfs_proto_tools.c -  A set of functions used to managed NFS.
 *
 *
 */

#ifndef _NFS_PROTO_TOOLS_H
#define _NFS_PROTO_TOOLS_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>           /* for having FNDELAY */
#include "HashData.h"
#include "HashTable.h"

#ifdef _USE_GSSRPC
#include <gssrpc/rpc.h>
#include <gssrpc/types.h>
#include <gssrpc/auth.h>
#include <gssrpc/pmap_clnt.h>
#else
#include <rpc/rpc.h>
#include <rpc/types.h>
#include <rpc/auth.h>
#include <rpc/pmap_clnt.h>
#endif

#include "log_functions.h"
#include "stuff_alloc.h"
#include "nfs23.h"
#include "nfs4.h"
#include "mount.h"
#include "cache_inode.h"
#include "nfs_tools.h"
#include "nfs_creds.h"
#include "nfs_file_handle.h"

/* type flag into mode field */
#define NFS2_MODE_NFDIR 0040000
#define NFS2_MODE_NFCHR 0020000
#define NFS2_MODE_NFBLK 0060000
#define NFS2_MODE_NFREG 0100000
#define NFS2_MODE_NFLNK 0120000
#define NFS2_MODE_NFNON 0140000

uint64_t nfs_htonl64(uint64_t arg64);
uint64_t nfs_ntohl64(uint64_t arg64);

cache_entry_t *nfs_FhandleToCache(u_long rq_vers,
                                  fhandle2 * pfh2,
                                  nfs_fh3 * pfh3,
                                  nfs_fh4 * pfh4,
                                  nfsstat2 * pstatus2,
                                  nfsstat3 * pstatus3,
                                  nfsstat4 * pstatus4,
                                  fsal_attrib_list_t * pattr,
                                  fsal_op_context_t * pcontext,
                                  cache_inode_client_t * pclient,
                                  hash_table_t * ht, int *prc);

void nfs_SetWccData(fsal_op_context_t * pcontext,
                    exportlist_t * pexport,
                    cache_entry_t * pentry,
                    fsal_attrib_list_t * pbefore_attr,
                    fsal_attrib_list_t * pafter_attr, wcc_data * pwcc_data);

int nfs_SetPostOpAttr(fsal_op_context_t * pcontext,
                      exportlist_t * pexport,
                      cache_entry_t * pentry,
                      fsal_attrib_list_t * pfsal_attr, post_op_attr * presult);

int nfs_SetPostOpXAttrDir(fsal_op_context_t * pcontext,
                          exportlist_t * pexport,
                          fsal_attrib_list_t * pfsal_attr, post_op_attr * presult);

int nfs_SetPostOpXAttrFile(fsal_op_context_t * pcontext,
                           exportlist_t * pexport,
                           fsal_attrib_list_t * pfsal_attr, post_op_attr * presult);

void nfs_SetPreOpAttr(fsal_attrib_list_t * pfsal_attr, pre_op_attr * pattr);

int nfs_RetryableError(cache_inode_status_t cache_status);

int nfs3_Sattr_To_FSAL_attr(fsal_attrib_list_t * pFSALattr, sattr3 * psattr);

void nfs_SetWccData(fsal_op_context_t * pcontext,
                    exportlist_t * pexport,
                    cache_entry_t * pentry,
                    fsal_attrib_list_t * pbefore_attr,
                    fsal_attrib_list_t * pafter_attr, wcc_data * pwcc_data);

void nfs_SetFailedStatus(fsal_op_context_t * pcontext,
                         exportlist_t * pexport,
                         int version,
                         cache_inode_status_t status,
                         nfsstat2 * pstatus2,
                         nfsstat3 * pstatus3,
                         cache_entry_t * pentry0,
                         post_op_attr * ppost_op_attr,
                         cache_entry_t * pentry1,
                         fsal_attrib_list_t * ppre_vattr1,
                         wcc_data * pwcc_data1,
                         cache_entry_t * pentry2,
                         fsal_attrib_list_t * ppre_vattr2, wcc_data * pwcc_data2);

#endif                          /* _NFS_PROTO_TOOLS_H */
