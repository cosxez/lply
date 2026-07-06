int lply_tcapt(void* sa)
{
	TSlply_capt* ts=(TSlply_capt*)sa;

	*(ts->ra)=lply_capt(ts->rmb,ts->rmbs,ts->mlist,ts->mlistrm,ts->tgmi,ts->tlmi,ts->flist,ts->flists,ts->tgfi,ts->mlisti,ts->mlistio,ts->is_busy,ts->ld,ts->mbuff,ts->sbuff,ts->mds,ts->eng,ts->decoder,ts->sound,ts->mcp,ts->sock,ts->faddr,ts->nls,ts->mfd);
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

int lply_clists(void *sa)
{
	TSlply_clists *ts=(TSlply_clists*)sa;
	if (*(ts->nstate)==1){if (lply_gmlfls(ts->sock,ts->faddr,ts->nmlist,ts->nmlists,ts->tnmi,ts->nflist,ts->nflists,ts->tnfi)!=0){*(ts->nstate)=0;}}
	if (*(ts->lstate)==1){lply_gmlfld(ts->lmlist,ts->lmlists,ts->tlmi,ts->lflist,ts->lflists,ts->tlfi,ts->lmd);}

	*(ts->gmlists)=*(ts->lmlists)+*(ts->nmlists);*(ts->tgmi)=*(ts->tlmi)+*(ts->tnmi);*(ts->gmlist)=(char*)malloc(*(ts->gmlists)+1);if (*(ts->lmlist)!=NULL && *(ts->lmlists)>0){memcpy(*(ts->gmlist),*(ts->lmlist),*(ts->lmlists));}if (*(ts->nmlist)!=NULL && *(ts->nmlists)>0){memcpy(*(ts->gmlist) + *(ts->lmlists),*(ts->nmlist),*(ts->nmlists));}
	*(ts->gflists)=*(ts->lflists)+*(ts->nflists);*(ts->tgfi)=*(ts->tlfi)+*(ts->tnfi);*(ts->gflist)=(char*)malloc(*(ts->gflists)+1);if (*(ts->lflist)!=NULL && *(ts->lflists)>0){memcpy(*(ts->gflist),*(ts->lflist),*(ts->lflists));}if (*(ts->nflist)!=NULL && *(ts->nflists)>0){memcpy(*(ts->gflist)+*(ts->lflists),*(ts->nflist),*(ts->nflists));}
	
	*(ts->rv)=1;
	free(ts);
	return 0;
}
