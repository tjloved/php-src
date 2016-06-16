--TEST--
Bug #25922 (SEGV in error_handler when context is destroyed)
--INI--
error_reporting=2047
--FILE--
<?php

function my_error_handler($error, $errmsg = '', $errfile = '', $errline = 0, $errcontext = '')
{
    echo "{$errmsg}\n";
    $errcontext = '';
}
function test()
{
    echo "Undefined index here: '{$data['HTTP_HEADER']}'\n";
}
function fn324131818()
{
    set_error_handler('my_error_handler');
    test();
}
fn324131818();
--EXPECT--
Undefined variable: data
Undefined index here: ''
