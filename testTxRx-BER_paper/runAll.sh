
# Exit immediately if a command fails
set -e

# Run TestTxRx 20 times

for VAR in {1..1}
do
	echo "Building environment..."
	cd environment
	make clean || true
	make
	cd ..

	echo "Building cache..."
	cd cache
	make clean || true
	make
	cd ..

	echo "Building testTxRx..."
	cd testTxRx 
	make clean || true
	make
	echo ""
	cd ..
    sleep 0.2
done

echo "Execution completed."

