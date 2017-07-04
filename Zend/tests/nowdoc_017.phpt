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
function fn1977956064()
{
    print "ok!\n";
}
fn1977956064();
--EXPECT--
ok!
