--TEST--
Bug #40899 (memory leak when nesting list())
--FILE--
<?php

function fn615370548()
{
    list(list($a, $b), $c) = array(array('a', 'b'), 'c');
    echo "{$a}{$b}{$c}\n";
}
fn615370548();
--EXPECT--
abc
