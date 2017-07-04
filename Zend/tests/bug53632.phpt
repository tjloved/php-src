--TEST--
zend_strtod() hangs with 2.2250738585072011e-308
--FILE--
<?php

function fn1318223181()
{
    $d = 2.225073858507201E-308;
    echo "Done\n";
}
fn1318223181();
--EXPECTF--	
Done
