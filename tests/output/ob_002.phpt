--TEST--
output buffering - ob_start
--FILE--
<?php

function fn458832058()
{
    ob_start();
    echo "foo\n";
}
fn458832058();
--EXPECT--
foo
