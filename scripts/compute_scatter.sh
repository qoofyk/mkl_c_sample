WORK_DIR=`pwd`
RESULT_DIR=${WORK_DIR}/results/`date +"compute_scatter_%y%m%d_%H%M"`
mkdir -pv ${RESULT_DIR}

BUILD_DIR=${WORK_DIR}/release
cd ${RESULT_DIR}


threads=(1 2 4 8 16 28)
N=256
loop_count=2000
bound=200
for i in `seq 1`
do
    #for((k=0; k<${#N[@]};k++)); do
    for k in `seq 72`
    do    
        for CASE_NAME in compute_bound
        do
        #export OMP_NUM_THREADS=${threads[k]}
        export OMP_NUM_THREADS=$k
        export KMP_AFFINITY=verbose,granularity=fine,scatter
        ${BUILD_DIR}/parallel_${CASE_NAME} -size $N -cnt $loop_count -bound $bound &>> ${RESULT_DIR}/log
        echo "No.$i exp of case ${CASE_NAME} done " &>> ${RESULT_DIR}/log
        done
    done
done
