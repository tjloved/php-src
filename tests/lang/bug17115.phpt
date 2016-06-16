--TEST--
Bug #17115 (lambda functions produce segfault with static vars)
--FILE--
<?php

function fn700056248()
{
    $func = create_function('', '
	static $foo = 0;
	return $foo++;
');
    var_dump($func());
    var_dump($func());
    var_dump($func());
}
fn700056248();
--EXPECT--
int(0)
int(1)
int(2)
