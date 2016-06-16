--TEST--
output buffering - nothing
--FILE--
<?php

function fn2025538372()
{
    echo "foo\n";
}
fn2025538372();
--EXPECT--
foo
