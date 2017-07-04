--TEST--
Bug #25547 (error_handler and array index with function call)
--FILE--
<?php

function handler($errno, $errstr, $errfile, $errline, $context)
{
    echo __FUNCTION__ . "({$errstr})\n";
}
function foo($x)
{
    return "foo";
}
function fn1686129327()
{
    set_error_handler('handler');
    $output = array();
    ++$output[foo("bar")];
    print_r($output);
    echo "Done";
}
fn1686129327();
--EXPECT--
handler(Undefined index: foo)
Array
(
    [foo] => 1
)
Done
