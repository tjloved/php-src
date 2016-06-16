--TEST--
Bug #54624 (class_alias and type hint)
--FILE--
<?php

class A
{
    function foo(A $param)
    {
    }
}
function fn2079572584()
{
    class_alias('A', 'AliasA');
    eval(' 
	class B extends A
	{
		function foo(AliasA $param) {
		}

	}
');
    echo "DONE\n";
}
fn2079572584();
--EXPECTF--
DONE
