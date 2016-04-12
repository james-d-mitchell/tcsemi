#define PTR_INC(p)	\
((p)>=CSTACKLIMIT?(p)=0:(p)++)
