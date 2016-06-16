--TEST--
Testing nowdoc in default value for property
--FILE--
<?php

class foo
{
    public $bar = <<<'EOT'
bar
EOT;
}
function fn2130497348()
{
    print "ok!\n";
}
fn2130497348();
--EXPECT--
ok!
