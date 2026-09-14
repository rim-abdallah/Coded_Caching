for VAR in {1..5}
do
    echo "Run $VAR" >> results.txt
    ./TestTxRx >> results.txt 2>&1
    echo "" >> results.txt
    sleep 0.2
done

