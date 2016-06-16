--TEST--
XORing booleans
--FILE--
<?php

function fn2075601176()
{
    $t = true;
    $f = false;
    var_dump($t ^ $f);
    var_dump($t ^ $t);
    var_dump($f ^ $f);
    echo "Done\n";
}
fn2075601176();
--EXPECTF--	
int(1)
int(0)
int(0)
Done
