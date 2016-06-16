--TEST--
XORing arrays
--FILE--
<?php

function fn638864731()
{
    $a = array(1, 2, 3);
    $b = array();
    $c = $a ^ $b;
    var_dump($c);
    echo "Done\n";
}
fn638864731();
--EXPECTF--	
int(1)
Done
