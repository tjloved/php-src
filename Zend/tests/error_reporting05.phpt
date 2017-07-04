--TEST--
testing @ and error_reporting - 5
--FILE--
<?php

class test
{
    function __get($name)
    {
        return $undef_name;
    }
    function __set($name, $value)
    {
        return $undef_value;
    }
}
function fn1549916015()
{
    error_reporting(E_ALL);
    $test = new test();
    $test->abc = 123;
    echo $test->bcd;
    @($test->qwe = 123);
    echo @$test->wer;
    var_dump(error_reporting());
    echo "Done\n";
}
fn1549916015();
--EXPECTF--	
Notice: Undefined variable: undef_value in %s on line %d

Notice: Undefined variable: undef_name in %s on line %d
int(32767)
Done
