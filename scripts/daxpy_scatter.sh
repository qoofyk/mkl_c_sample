WORK_DIR=`pwd`
RESULT_DIR=${WORK_DIR}/results/`date +"daxpy_scatter_%y%m%d_%H%M"`
mkdir -pv ${RESULT_DIR}

BUILD_DIR=${WORK_DIR}/release
cd ${RESULT_DIR}


#threads=(1 2 4 8 16 28)
N=$((1024*1024))
loop_count=1000
#step=256
for i in `seq 1`
do
    #for((k=0; k<${#N[@]};k++)); do
    for k in `seq $max_threads`
    do    
        for CASE_NAME in daxpy
        do
        #export OMP_NUM_THREADS=${threads[k]}
        export OMP_NUM_THREADS=$k
        export KMP_AFFINITY=verbose,granularity=fine,scatter
        ${BUILD_DIR}/parallel_${CASE_NAME} -size $N -iter $loop_count &>> ${RESULT_DIR}/log
        echo "No.$i threads=$k exp of case ${CASE_NAME} loop_cnt=$loop_count done " &>> ${RESULT_DIR}/log
        echo &>> ${RESULT_DIR}/log
        echo &>> ${RESULT_DIR}/log
        echo &>> ${RESULT_DIR}/log
        done
    done
done
