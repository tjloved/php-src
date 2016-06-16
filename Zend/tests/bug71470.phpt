--TEST--
Bug #71470: Leaked 1 hashtable iterators
--FILE--
<?php

function fn64373584()
{
    $array = [1, 2, 3];
    foreach ($array as &$v) {
        die("foo\n");
    }
}
fn64373584();
--EXPECT--
foo
