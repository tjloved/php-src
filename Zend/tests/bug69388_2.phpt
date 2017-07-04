--TEST--
Bug #69388 - Variation
--FILE--
<?php

function handle_error($code, $message, $file, $line, $context)
{
    eval('namespace Foo;');
    echo "{$message}\n";
}
function fn824951928()
{
    error_reporting(E_ALL | E_STRICT);
    set_error_handler('handle_error');
    eval('namespace {use Exception;}');
}
fn824951928();
--EXPECT--
The use statement with non-compound name 'Exception' has no effect
