/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef NF_RETC_H323_JOURNAL_H
#define NF_RETC_H323_JOURNAL_H
#include "nf_retc_h323_portable.h"
/* H323.B research only: exactly one portable two-actor metadata record per
 * file. No A6 publication, B2 issuer, B3 application, checkpoint or WAL tail.
 * Writer is single-owner and caller serializes writers. */
int nf_h323_journal_write(const char *path,const NfH323Record *r);
int nf_h323_journal_read(const char *path,NfH323Record *out);
#endif
