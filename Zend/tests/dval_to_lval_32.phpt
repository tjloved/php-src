--TEST--
zend_dval_to_lval preserves low bits  (32 bit long)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4)
	 die("skip for machines with 32-bit longs");
?>
--FILE--
<?php

function fn595913699()
{
    /* test doubles around -4e21 */
    $values = [-4.000000000000001E+21, -4.0000000000000005E+21, -4.0E+21, -3.9999999999999995E+21, -3.999999999999999E+21];
    /* see if we're rounding negative numbers right */
    $values[] = -2147483649.8;
    foreach ($values as $v) {
        var_dump((int) $v);
    }
}
fn595913699();
--EXPECT--
int(-2056257536)
int(-2055733248)
int(-2055208960)
int(-2054684672)
int(-2054160384)
int(2147483647)
