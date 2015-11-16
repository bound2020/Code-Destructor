awk -f mae.awk $1
sort -nrk3 $1 | awk -f auc.awk
