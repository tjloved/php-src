--TEST--
output buffering - nothing
--FILE--
<?php

function fn1592010918()
{
    echo "foo\n";
}
fn1592010918();
--EXPECT--
foo
