--TEST--
Bug #69388: Use after free on recursive calls to PHP compiler
--FILE--
<?php

function handle_error($code, $message, $file, $line, $context)
{
    if (!function_exists("bla")) {
        eval('function bla($s) {echo "$s\\n";}');
    }
    bla($message);
}
function fn1355247272()
{
    error_reporting(E_ALL | E_STRICT);
    set_error_handler('handle_error');
    eval('namespace {use Exception;}');
}
fn1355247272();
--EXPECT--
The use statement with non-compound name 'Exception' has no effect
