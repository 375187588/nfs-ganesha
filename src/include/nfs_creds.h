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
 */

/**
 * \file    nfs_creds.h
 * \author  $Author: deniel $
 * \date    $Date: 2005/11/28 17:03:23 $
 * \version $Revision: 1.4 $
 * \brief   Prototypes for the RPC credentials used in NFS.
 *
 * nfs_creds.h : Prototypes for the RPC credentials used in NFS.
 *
 *
 */

#ifndef _NFS_CREDS_H
#define _NFS_CREDS_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/param.h>

#ifdef _USE_GSSRPC
#include <gssapi/gssapi.h>
#include <gssapi/gssapi_krb5.h>

#include <gssrpc/rpc.h>
#else
#include <rpc/rpc.h>
#endif

#include "LRU_List.h"
#include "fsal.h"
#include "cache_inode.h"
#include "cache_content.h"

#include "nfs23.h"
#include "nfs4.h"
#include "mount.h"

#include "err_LRU_List.h"
#include "err_HashTable.h"
#include "err_rpc.h"

typedef enum CredType__ { CRED_NONE = 1, CRED_UNIX = 2, CRED_GSS = 3 } CredType_t;

typedef struct CredUnix__ {
  u_int uid;
  u_int gid;
  /* May be we could had list of groups management */
} CredUnix_t;

typedef struct CredGss__ {
#if(  defined( HAVE_KRB5 ) && defined ( _USE_GSSRPC ) )
  gss_qop_t qop;
  gss_OID mech;
  rpc_gss_svc_t svc;
  gss_ctx_id_t context;
#else
  int dummy;
#endif
} CredGss_t;

typedef union CredData__ {
#ifdef HAVE_KRB5
  CredUnix_t unix_cred;
  CredGss_t gss_cred;
#else
  int dummy;
#endif
} CredData_t;

typedef struct RPCSEC_GSS_cred__ {
  CredType_t type;
  CredData_t data;
} RPCSEC_GSS_cred_t;

#endif                          /* _NFS_CREDS_H */
