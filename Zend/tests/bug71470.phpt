--TEST--
Bug #71470: Leaked 1 hashtable iterators
--FILE--
<?php

function fn1423561756()
{
    $array = [1, 2, 3];
    foreach ($array as &$v) {
        die("foo\n");
    }
}
fn1423561756();
--EXPECT--
foo
