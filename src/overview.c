/*    (^=__=^)
 *    ~Overview~                      
 *                                      
 *  
 *  TODO: fix the menu jank (x on side of box when change setting)
 *  TODO: reset to default controls
 *  TODO: fix input display clearing after menu open (where is the cursor before closing?)
 *  TODO: make soft reset, resetting like it is now is bad.
 *
 *  TODO: choose predefined schemes 
 *  TODO: Test time limit mechanic for think/exec
 *  TODO: Implement fail states
 *     
 *       
 *                                    [complexity][time]  
 *  FUNDAMENTAL MECHANICS    
 *     |_scoring                      [medium]    [mid]   <<< maybe later
 *     \_combos and B2Bs              [medium]    [mid]   <<< maybe laterer
 *
 *  PLANTRIS MECHANICS                Done       
 *     |_plan phase:
 *     |     |_text interface         
 *     |     |_based on finesse
 *     |     \_need rapid prototypes      
 *     |     
 *     |_execute phase: uses
 *       fundamentals, replay
 *       a sequence of inputs
 *
 *  MENU SYSTEM                       Done       
 *     |_board/window states          [hard?]     [long] 
 *     |_used for tutorials,          [indeed]    [yes]  <<< Bare minimum tutorial
 *     |   plan-execute loop         
 *     \_transitions                  [medium]    [long] <<< No
 *
 *  AESTHETICS                     <<<No   
 *     |_little window for         
 *     | combo/clear display            
 *     | w/sick animations
 *     | when you TSD
 *     \_way to do nice easeing
 *       on screen transitions     
 *  
 *  PROBABLYS
 *     \_40l sprint
 *                                    
 *  MAYBES                            
 *     |_live undo                    [hard]      [long]  <<< No
 *     |_finesse logic                [hard]      [long]  <<< No
 *     \_customize themes             [maybe]     [mid]   <<< No
 *                                    
 *  PROBABLY NOTS                                           <<< lol 
 *     |_solvers(find t-spin          [ultra]     [forever] 
 *     | setups? doubles? efficiency?)
 *     \_xinput mode for key          [basically impossible]
 *         release, DAS, ARR
 *
 */
