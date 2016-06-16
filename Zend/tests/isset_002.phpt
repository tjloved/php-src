--TEST--
Testing isset with several undefined variables as argument
--FILE--
<?php

function fn949493799()
{
    var_dump(isset($a, ${$b}, ${$c}, ${${${$d}}}, $e[$f->g]->d));
}
fn949493799();
--EXPECT--
bool(false)
