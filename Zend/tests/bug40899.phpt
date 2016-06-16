--TEST--
Bug #40899 (memory leak when nesting list())
--FILE--
<?php

function fn1562680344()
{
    list(list($a, $b), $c) = array(array('a', 'b'), 'c');
    echo "{$a}{$b}{$c}\n";
}
fn1562680344();
--EXPECT--
abc
