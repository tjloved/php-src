--TEST--
Testing isset with several undefined variables as argument
--FILE--
<?php

function fn1608099011()
{
    var_dump(isset($a, ${$b}, ${$c}, ${${${$d}}}, $e[$f->g]->d));
}
fn1608099011();
--EXPECT--
bool(false)
