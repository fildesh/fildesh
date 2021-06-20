# Git Patterns

## Branching Out

```shell
git branch mybranch
git checkout mybranch
git push origin mybranch
```

## Merging Back

```shell
# Merge from trunk first.
git checkout mybranch
# I like to see the changes and test before committing.
# Fast-forward works too: `git merge --ff-only trunk`
# The branch's history will not be preserved when merging to trunk anyway.
git merge --no-ff --no-commit trunk
git commit -S

# Merge back to trunk.
git checkout trunk
git merge --squash mybranch
git commit -S
git push origin trunk

# Then delete the branch.
git branch -d mybranch
git push origin --delete mybranch
```
