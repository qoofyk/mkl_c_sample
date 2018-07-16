WORK_DIR=`pwd`
RESULT_DIR=${WORK_DIR}/results/`date +"memory_compact_%y%m%d_%H%M"`
mkdir -pv ${RESULT_DIR}

BUILD_DIR=${WORK_DIR}/release
cd ${RESULT_DIR}


#threads=(1 2 4 8 16 28)
N=1024
loop_count=1000
#step=256
for i in `seq 1`
do
    #for((k=0; k<${#N[@]};k++)); do
    for k in `seq $max_num_threads`
    do    
        for CASE_NAME in memory_bound
        do
        #export OMP_NUM_THREADS=${threads[k]}
        export OMP_NUM_THREADS=$k
        export KMP_AFFINITY=verbose,granularity=fine,compact,1,0
        ${BUILD_DIR}/parallel_${CASE_NAME} -size $N -cnt $loop_count -step $step &>> ${RESULT_DIR}/log
        echo "No.$i exp of case ${CASE_NAME} loop_cnt=$loop_count step=$step done " &>> ${RESULT_DIR}/log
        done
    done
done
