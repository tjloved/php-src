--TEST--
testing @ and error_reporting - 10
--FILE--
<?php

function make_exception()
{
    @$blah;
    str_replace();
    error_reporting(0);
    throw new Exception();
}
function fn1030307136()
{
    error_reporting(E_ALL);
    try {
        @make_exception();
    } catch (Exception $e) {
    }
    var_dump(error_reporting());
    error_reporting(E_ALL & ~E_NOTICE);
    try {
        @make_exception();
    } catch (Exception $e) {
    }
    var_dump(error_reporting());
    echo "Done\n";
}
fn1030307136();
--EXPECTF--	
int(32767)
int(32759)
Done
