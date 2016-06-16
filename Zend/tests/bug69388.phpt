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
function fn462820647()
{
    error_reporting(E_ALL | E_STRICT);
    set_error_handler('handle_error');
    eval('namespace {use Exception;}');
}
fn462820647();
--EXPECT--
The use statement with non-compound name 'Exception' has no effect
