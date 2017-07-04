--TEST--
Ensure __autoload() is triggered during unserialization.
--FILE--
<?php

function fn2106423171()
{
    spl_autoload_register(function ($name) {
        echo "in autoload: {$name}\n";
    });
    var_dump(unserialize('O:1:"C":0:{}'));
}
fn2106423171();
--EXPECTF--
in autoload: C
object(__PHP_Incomplete_Class)#%d (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(1) "C"
}
