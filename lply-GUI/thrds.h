int lply_tcapt(void* sa)
{
	TSlply_capt* ts=(TSlply_capt*)sa;

	*(ts->ra)=lply_capt(ts->rmb,ts->rmbs,ts->mlist,ts->mlistrm,ts->tgmi,ts->tlmi,ts->flist,ts->flists,ts->tgfi,ts->mlisti,ts->mlistio,ts->is_busy,ts->ld,ts->mbuff,ts->sbuff,ts->mds,ts->eng,ts->decoder,ts->sound,ts->mcp,ts->sock,ts->faddr,ts->nls);
	free(ts);
	return 0;
}

int lply_tptfr(void* sa)
{
	TSlply_ptfr* ts=(TSlply_ptfr*)sa;

	lply_ptfr(ts->rmb,ts->rmbs,ts->tidx,ts->mbuff,ts->sbuff,ts->mds,ts->mcp,ts->eng,ts->sound,ts->decoder,ts->is_busy);
	free(ts);
	return 0;
}
