function [is_converged] = kmeans_multilabel_callback(it, x, y, ny, ...
                                                      nx, L, C)

    result.x = x;
    result.y = y;
    u = prost.variable(nx*ny*L);
    prost.get_all_variables(result, {u}, {}, {}, {});
    
    u = reshape(u.val, [ny nx L]);
    u_colors = zeros(ny,nx,3);
    for i = 1:L
        u_colors = u_colors + u(:,:,i).*reshape(C(i,:),1,1,3);
    end
    imshow(u_colors);

    
    is_converged = false;
    
    fprintf('\n');
    
end

