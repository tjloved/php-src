--TEST--
Bug #31341 (escape on curly inconsistent)
--FILE--
<?php

function fn133187179()
{
    $a = array("\$     \\{    ", "      \\{   \$", "      \\{\$   ", "      \$\\{   ", "      \$\\{  ", "      \\{\$  ", "\$    \\{    ", "      \\{  \$", "%     \\{    ");
    foreach ($a as $v) {
        echo "'{$v}'\n";
    }
}
fn133187179();
--EXPECT--
'$     \{    '
'      \{   $'
'      \{$   '
'      $\{   '
'      $\{  '
'      \{$  '
'$    \{    '
'      \{  $'
'%     \{    '
