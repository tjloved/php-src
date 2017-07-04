--TEST--
XORing arrays
--FILE--
<?php

function fn973692223()
{
    $a = array(1, 2, 3);
    $b = array();
    $c = $a ^ $b;
    var_dump($c);
    echo "Done\n";
}
fn973692223();
--EXPECTF--	
int(1)
Done
