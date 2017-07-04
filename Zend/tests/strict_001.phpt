--TEST--
using resource as array offset
--INI--
error_reporting=8191
--FILE--
<?php

function fn1042331662()
{
    $fp = fopen(__FILE__, 'r');
    $array = array(1, 2, 3, 4, 5, 6, 7);
    var_dump($array[$fp]);
    echo "Done\n";
}
fn1042331662();
--EXPECTF--	
Notice: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
int(%d)
Done
