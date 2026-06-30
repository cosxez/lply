int lply_tcapt(void* sa)
{
	TSlply_capt* ts=(TSlply_capt*)sa;

	*(ts->ra)=lply_capt(ts->mlist,ts->mlistrm,ts->tgmi,ts->tlmi,ts->flist,ts->flists,ts->tgfi,ts->mlisti,ts->mlistio,ts->is_busy,ts->ld,ts->mbuff,ts->sbuff,ts->eng,ts->decoder,ts->sound,ts->mcp,ts->sock,ts->faddr,ts->nls);
	free(ts);
	return 0;
}
