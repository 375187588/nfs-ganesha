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
 * nfs_remote_functions.h : Prototypes for NFS protocol functions through RPCs. 
 *
 *
 */

#ifndef _NFS_REMOTE_FUNCTIONS_H
#define _NFS_REMOTE_FUNCTIONS_H

#include "nfs_proto_functions.h"
#ifdef _USE_GSSRPC
#include <gssrpc/clnt.h>
#else
#include <rpc/clnt.h>
#endif

/**
 * @defgroup MNTprocs    Mount protocol functions.
 * 
 * @{
 */
int mnt1_remote_Null(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int mnt1_remote_Mnt(CLIENT * clnt /* IN  */ ,
                    nfs_arg_t * parg /* IN  */ ,
                    nfs_res_t * pres /* OUT */ );

int mnt1_remote_Dump(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int mnt1_remote_Umnt(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int mnt1_remote_UmntAll(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int mnt1_remote_Export(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int mnt3_remote_Null(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int mnt3_remote_Mnt(CLIENT * clnt /* IN  */ ,
                    nfs_arg_t * parg /* IN  */ ,
                    nfs_res_t * pres /* OUT */ );

int mnt3_remote_Dump(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int mnt3_remote_Umnt(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int mnt3_remote_UmntAll(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int mnt3_remote_Export(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

/* @}
 * -- End of MNT protocol functions. --
 */

/**
 * @defgroup NFSprocs    NFS protocols functions.
 * 
 * @{
 */

int nfs2_remote_Null(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int nfs2_remote_Getattr(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int nfs2_remote_Setattr(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int nfs2_remote_Root(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int nfs2_remote_Lookup(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs2_remote_Readlink(CLIENT * clnt /* IN  */ ,
                         nfs_arg_t * parg /* IN  */ ,
                         nfs_res_t * pres /* OUT */ );

int nfs2_remote_Read(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int nfs2_remote_Writecache(CLIENT * clnt /* IN  */ ,
                           nfs_arg_t * parg /* IN  */ ,
                           nfs_res_t * pres /* OUT */ );

int nfs2_remote_Write(CLIENT * clnt /* IN  */ ,
                      nfs_arg_t * parg /* IN  */ ,
                      nfs_res_t * pres /* OUT */ );

int nfs2_remote_Create(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs2_remote_Remove(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs2_remote_Rename(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs2_remote_Link(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int nfs2_remote_Symlink(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int nfs2_remote_Mkdir(CLIENT * clnt /* IN  */ ,
                      nfs_arg_t * parg /* IN  */ ,
                      nfs_res_t * pres /* OUT */ );

int nfs2_remote_Rmdir(CLIENT * clnt /* IN  */ ,
                      nfs_arg_t * parg /* IN  */ ,
                      nfs_res_t * pres /* OUT */ );

int nfs2_remote_Readdir(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int nfs2_remote_Fsstat(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs3_remote_Null(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int nfs3_remote_Getattr(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int nfs3_remote_Setattr(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int nfs3_remote_Lookup(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs3_remote_Readlink(CLIENT * clnt /* IN  */ ,
                         nfs_arg_t * parg /* IN  */ ,
                         nfs_res_t * pres /* OUT */ );

int nfs3_remote_Read(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int nfs3_remote_Write(CLIENT * clnt /* IN  */ ,
                      nfs_arg_t * parg /* IN  */ ,
                      nfs_res_t * pres /* OUT */ );

int nfs3_remote_Create(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs3_remote_Remove(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs3_remote_Rename(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs3_remote_Link(CLIENT * clnt /* IN  */ ,
                     nfs_arg_t * parg /* IN  */ ,
                     nfs_res_t * pres /* OUT */ );

int nfs3_remote_Symlink(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int nfs3_remote_Mkdir(CLIENT * clnt /* IN  */ ,
                      nfs_arg_t * parg /* IN  */ ,
                      nfs_res_t * pres /* OUT */ );

int nfs3_remote_Rmdir(CLIENT * clnt /* IN  */ ,
                      nfs_arg_t * parg /* IN  */ ,
                      nfs_res_t * pres /* OUT */ );

int nfs3_remote_Readdir(CLIENT * clnt /* IN  */ ,
                        nfs_arg_t * parg /* IN  */ ,
                        nfs_res_t * pres /* OUT */ );

int nfs3_remote_Fsstat(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs3_remote_Access(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs3_remote_Readdirplus(CLIENT * clnt /* IN  */ ,
                            nfs_arg_t * parg /* IN  */ ,
                            nfs_res_t * pres /* OUT */ );

int nfs3_remote_Fsinfo(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs3_remote_Pathconf(CLIENT * clnt /* IN  */ ,
                         nfs_arg_t * parg /* IN  */ ,
                         nfs_res_t * pres /* OUT */ );

int nfs3_remote_Commit(CLIENT * clnt /* IN  */ ,
                       nfs_arg_t * parg /* IN  */ ,
                       nfs_res_t * pres /* OUT */ );

int nfs3_remote_Mknod(CLIENT * clnt /* IN  */ ,
                      nfs_arg_t * parg /* IN  */ ,
                      nfs_res_t * pres /* OUT */ );

int nfs4_remote_COMPOUND(CLIENT * clnt /* IN  */ ,
                         COMPOUND4args * parg /* IN  */ ,
                         COMPOUND4res * pres /* OUT */ );

int nfs4_remote_Null(CLIENT * clnt /* IN  */ );

/* Free functions */
void mnt1_remote_Mnt_Free(nfs_res_t * pres);
void mnt1_remote_Dump_Free(nfs_res_t * pres);
void mnt1_remote_Export_Free(nfs_res_t * pres);
void mnt1_remote_Null_Free(nfs_res_t * pres);
void mnt1_remote_Umnt_Free(nfs_res_t * pres);
void mnt1_remote_UmntAll_Free(nfs_res_t * pres);

void mnt3_remote_Mnt_Free(nfs_res_t * pres);
void mnt3_remote_Dump_Free(nfs_res_t * pres);
void mnt3_remote_Export_Free(nfs_res_t * pres);
void mnt3_remote_Null_Free(nfs_res_t * pres);
void mnt3_remote_Umnt_Free(nfs_res_t * pres);
void mnt3_remote_UmntAll_Free(nfs_res_t * pres);

void nfs2_remote_Null_Free(nfs_res_t * resp);
void nfs2_remote_Getattr_Free(nfs_res_t * resp);
void nfs2_remote_Setattr_Free(nfs_res_t * resp);
void nfs2_remote_Lookup_Free(nfs_res_t * resp);
void nfs2_remote_Read_Free(nfs_res_t * resp);
void nfs2_remote_Write_Free(nfs_res_t * resp);
void nfs2_remote_Create_Free(nfs_res_t * resp);
void nfs2_remote_Mkdir_Free(nfs_res_t * resp);
void nfs2_remote_Symlink_Free(nfs_res_t * resp);
void nfs2_remote_Remove_Free(nfs_res_t * resp);
void nfs2_remote_Rmdir_Free(nfs_res_t * resp);
void nfs2_remote_Rename_Free(nfs_res_t * resp);
void nfs2_remote_Link_Free(nfs_res_t * resp);
void nfs2_remote_Fsstat_Free(nfs_res_t * resp);
void nfs2_remote_Root_Free(nfs_res_t * pres);
void nfs2_remote_Writecache_Free(nfs_res_t * pres);
void nfs2_remote_Readdir_Free(nfs_res_t * resp);
void nfs2_remote_Readlink_Free(nfs_res_t * resp);

void nfs3_remote_Null_Free(nfs_res_t * resp);
void nfs3_remote_Getattr_Free(nfs_res_t * resp);
void nfs3_remote_Setattr_Free(nfs_res_t * resp);
void nfs3_remote_Lookup_Free(nfs_res_t * resp);
void nfs3_remote_Access_Free(nfs_res_t * pres);
void nfs3_remote_Readlink_Free(nfs_res_t * resp);
void nfs3_remote_Write_Free(nfs_res_t * resp);
void nfs3_remote_Create_Free(nfs_res_t * resp);
void nfs3_remote_Mkdir_Free(nfs_res_t * resp);
void nfs3_remote_Symlink_Free(nfs_res_t * resp);
void nfs3_remote_Mknod_Free(nfs_res_t * pres);
void nfs3_remote_Remove_Free(nfs_res_t * resp);
void nfs3_remote_Rmdir_Free(nfs_res_t * resp);
void nfs3_remote_Rename_Free(nfs_res_t * resp);
void nfs3_remote_Link_Free(nfs_res_t * resp);
void nfs3_remote_Readdir_Free(nfs_res_t * resp);
void nfs3_remote_Readdirplus_Free(nfs_res_t * resp);
void nfs3_remote_Fsstat_Free(nfs_res_t * resp);
void nfs3_remote_Fsinfo_Free(nfs_res_t * pres);
void nfs3_remote_Pathconf_Free(nfs_res_t * pres);
void nfs3_remote_Commit_Free(nfs_res_t * pres);
void nfs3_remote_Read_Free(nfs_res_t * resp);

#endif
