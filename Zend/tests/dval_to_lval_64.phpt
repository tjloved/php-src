--TEST--
zend_dval_to_lval preserves low bits  (64 bit long)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8)
	 die("skip for machines with 64-bit longs");
?>
--FILE--
<?php

function fn1078405587()
{
    /* test doubles around -4e21 */
    $values = [-4.000000000000001E+21, -4.0000000000000005E+21, -4.0E+21, -3.9999999999999995E+21, -3.999999999999999E+21];
    foreach ($values as $v) {
        var_dump((int) $v);
    }
}
fn1078405587();
--EXPECT--
int(2943463994971652096)
int(2943463994972176384)
int(2943463994972700672)
int(2943463994973224960)
int(2943463994973749248)
