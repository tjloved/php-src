--TEST--
complex cases of variable assignment - 001
--FILE--
<?php

function fn229621259()
{
    $var = array(1, 2, 3);
    $var1 =& $var;
    $var = $var[1];
    var_dump($var);
    var_dump($var1);
    echo "Done\n";
}
fn229621259();
--EXPECTF--	
int(2)
int(2)
Done
