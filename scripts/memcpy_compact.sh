WORK_DIR=`pwd`
RESULT_DIR=${WORK_DIR}/results/`date +"memcpy_%y%m%d_%H%M"`
mkdir -pv ${RESULT_DIR}

BUILD_DIR=${WORK_DIR}/release
cd ${RESULT_DIR}


#threads=(1 2 4 8 16 28)
#N=(16 32 64 128 256 512 768 1024 1280 1536 1792 2048 2034 2560)
#N=(256 512 768 1024 1280 1536 1792 2048 2034 2560)
N=1024
loop_count=20000

for i in `seq 1`
do
    #for((k=0; k<${#N[@]};k++)); 
    for k in `seq $max_threads`
    do    
        for CASE_NAME in memcpy
        do
        #export OMP_NUM_THREADS=${threads[k]}
        export OMP_NUM_THREADS=$k
        export KMP_AFFINITY=verbose,granularity=fine,compact
        ${BUILD_DIR}/parallel_${CASE_NAME} -size $N -iter $loop_count &>> ${RESULT_DIR}/log
        echo "No.$i exp of case ${CASE_NAME} done " &>> ${RESULT_DIR}/log
        done
    done
done
