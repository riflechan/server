perl mysql-test-run.pl --verbose-restart --force --testcase-timeout=450000 --suite-timeout=600000 --max-test-fail=500 --retry=3  --parallel=4 --suite=^
main,^
innodb,^
plugins,^
mariabackup,^
rocksdb
